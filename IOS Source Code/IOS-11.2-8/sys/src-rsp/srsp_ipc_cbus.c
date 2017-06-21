/* $Id: srsp_ipc_cbus.c,v 3.6.6.7 1996/08/28 13:14:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/srsp_ipc_cbus.c,v $
 *------------------------------------------------------------------
 * RSP CBUS transport-specific routines at slave side.
 *
 * June 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srsp_ipc_cbus.c,v $
 * Revision 3.6.6.7  1996/08/28  13:14:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.6  1996/08/15  15:32:13  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.6.6.5  1996/07/23  13:28:48  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.6.4  1996/06/06  21:43:01  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * Fix populate_txq_txacc_to_ips not defined in boot image
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/06/04  05:04:35  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/05/13  22:34:12  skufer
 * CSCdi56336:  Memory Parity error-HSA Slave dies--box hangs
 * Branch: California_branch
 *
 * Revision 3.6.6.1  1996/03/18  22:02:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  07:36:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/07  10:45:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6  1996/03/06  19:13:09  foster
 * CSCdi49134:  HSA : OIR event causes router to crash
 * don't reset the slave if it is running durning oir
 *
 * Revision 3.5  1996/02/28  09:33:55  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.4  1996/01/22  07:15:58  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/27  21:34:12  yanke
 * Use cbus protection mechanism to guard slave rsp from crashing
 * during cbus stalled condition.
 * Recover ipc operation on master rsp when stall condition goes away.
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 * Revision 3.2  1995/11/17  18:50:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/04  01:36:27  yanke
 * Initial commit of placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SRSP_MC_ASIC_TIME 42   /* must wait at least 41uS for MC_ASIC bug */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "packet.h"
#include "interface.h"
#include "sys_registry.h"
#include "subsys.h"

#include "../hes/if_fci.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/ucode.h"      
#include "../hes/ccb_cmd.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/if_cmd.h"
#include "../hes/cbus_registry.h"

#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"

#include "rsp_bufhdr.h"
#include "rsp_chasint.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "rsp_pcmap.h"
#include "qa_hw.h"
#include "rsp_fast.h"
#include "rsp_init.h"
#include "rsp_cache.h"

#include "rsp_ipc_private.h"
#include "rsp_ipc_cbus.h"
#include "rsp_cbus_monitor.h"

extern boolean cbus_monitor_debug;

/*
 * Global IPC structure for RSP slave
 */
typedef struct rsp_slave_ipc_rec_ 
{
    boolean valid;			/* indicate if structure is valid */
    ipc_seat seat_id;			/* slave's seat id */
    ipc_port_id master_ctrl_port_id;	/* master control port id */
    qa_hwq_t *master_rcv_hwq;		/* master's receive h/w queue */
    qa_hwq_t *self_rcv_hwq;		/* my own receive h/w queue */
    qa_hwq_t *ipc_free_hwq;		/* free queue to get free buffer */
    qa_hwq_t *cmdq;			/* command queue */
    ipc_card_t *own_transport;		/* my own transport structure */
} rsp_slave_ipc_rec_t;

static rsp_slave_ipc_rec_t ipc_rec;
static rsp_slave_ipc_rec_t *ipcp = &ipc_rec;
static boolean ipc_cbus_error_notice;

static const uchar rsp_self_slot = MAX_SLOTS;


/*
 * Function pointer to polling function.
 */
typedef int (* ipc_proc_fn_t)(void);

ipc_proc_fn_t ipc_proc_fn = NULL;

watched_boolean* ipc_process_wakeup;

/*
 * Return the card structure of my own.
 */
ipc_card_t *get_own_transport (void)
{
    return ipcp->own_transport;
}

/*
 * Allocate a memd buffer from IPC global
 * free queue.
 */
bufhdr_t *alloc_ipc_memd_buffer (ulong ipc_control)
{
    bufhdr_t *bufhdr;
    qa_hwq_t *qptr;

    qptr = ipcp->ipc_free_hwq;
    bufhdr = hwq_dequeue(qptr);
    if (rsp_cbus_check_read() == FALSE) {
        if (cbus_monitor_debug)
          buginf("\nalloc_ipc_memd_buffer failed on Cbus read of hwq");
        return(NULL);
    }

    if (bufhdr) {
	/*
	 * Uses uncached buffer address, since we'll only read
	 * one field in the header (bf_buffer_ptr), which is cheaper
	 * than invalidating it and pulling in a whole cache line.
	 */
	bufhdr = ADDR_MEMD_TO_UNCACHED(bufhdr);

	/*
	 * Set up to have buffer returned to global free queue
	 * when done.
	 */
	bufhdr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(qptr);
		
	/*
	 * Also save global free queue address in case we later
	 * decide to have the buffer post processed on the return
	 * queue.
	 */
	bufhdr->bf_freeq_ptr = ADDR_TO_MEMD_OFFSET_16(qptr);
    }
    return(bufhdr);
}

/*
 * Send registration message back to master to its init port.
 */
static int send_reg_message (void)
{
#ifdef YANKE
    paktype *pak;
    ipc_cbus_reg_msg_t *regp;
    ipc_port_id portid;
    ipc_error_code retcode;

    pak = getbuffer(sizeof(ipc_cbus_reg_msg_t));
    if (pak == NULL) {
	return -1;
    }

    /*
     * Fill in the message header content.
     */
    regp = (ipc_cbus_reg_msg_t *)pak->datagramstart;
    regp->seat_id = ipcp->seat_id;

    /*
     * Find the master's init port id.
     */
    portid = ipc_locate_port(IPC_CBUS_INIT_PORT_NAME);
    if (portid == 0) {
	errmsg(&msgsym(MSGERR, IPC_RSP_CBUS),
	    "Can not locate master's init port", portid);
	return -1;
    }

    /*
     * Send this message to master's init port.
     */
    retcode = ipc_send_pak_async(pak, portid, 0, NULL, NULL);
    if (retcode != IPC_OK) {
	errmsg(&msgsym(MSGERR, IPC_RSP_CBUS),
	    "Can not send message via IPC", (int)retcode);
	return -1;
    }
#endif
    return 0;
}

/*
 * This routine is called in the interrupt context.
 */
static void cbus_unavailable_notify (void)
{
    /*
     * We are being notified that cbus is bad.
     */
    ipc_cbus_error_notice = TRUE;
    return;
}

static void cbus_available_notify (void)
{
    if (cbus_monitor_debug)
	buginf("\nNotify from monitor, CBus is alive!");
    /*
     * We are being notified that the cbus is good again.
     * All we have to do is to wake up our process.
     */
    process_set_boolean(ipc_process_wakeup, TRUE);
}

/*
 * Message queue polling routine.
 */
static int ipc_proc_message (void)
{
    qa_hwq_t *qptr;
    bufhdr_t *bp;
    int size;
    paktype *pak;
    char *bufp;
    ulong status;
    boolean cbus_ok;

    /*
     * Make sure we have a valid queue to poll.
     */
    qptr = ipcp->self_rcv_hwq;
    if (qptr == NULL)
	return 0;

    /*
     * MC ASIC WORKAROUND
     *     mcasic may cause us to crash with data cache parity error
     *     if we access in exactly 2^11 pipeline cycles from the prev
     *     access.  2048 cycles * 20ns/cycle = 40.960 microseconds.
     *     Delay 50us before turning off interrupts so we may 
     *     not hit this critical region including any writes posted
     *     in the write buffer going across the cy bus.
     */
    usecdelay(SRSP_MC_ASIC_TIME); 
    GLOBAL_INTERRUPT_BLOCK(status);
    bp = hwq_dequeue(qptr);

    /*
     * If anything happened when we access
     * the hw queue, return -1 to inform our caller.
     */

    if (rsp_cbus_check_read() == FALSE) {
	GLOBAL_INTERRUPT_UNBLOCK(status);
	return -1;
    }

    if (bp == NULL) {
	GLOBAL_INTERRUPT_UNBLOCK(status);
	return 0;
    }

    if (cbus_monitor_debug)
	buginf("\nGet an ipc buffer %x from hwq %x", (uint)bp, (uint)qptr);


    /*
     * Invalidate the buffer header so we get
     * fresh data from the data cache.
     */
    inval_buf_hdr_inline((bufhdr_t *)bp);

    /*
     * Get a free system buffer of appropriate size.
     * If can not, drop the memd buffer and bail out.
     * We use the bf_tx_bytes as this buffer isn't
     * given to us by microcode. It contains the
     * same content as it was transmitted.
     */
    size = bp->bf_tx_bytes;
    bufp = (char *)ADDR_MEMD_TO_UNCACHED(bp->bf_buffer_ptr);
    GLOBAL_INTERRUPT_UNBLOCK(status);

    /* Ensure that we get a valid pointer. */
    if (rsp_cbus_check_read() == FALSE) {
	return -1;
    }

    pak = getbuffer(size);
    if (pak == NULL) {
        /*
	 * protect return of buffer for slave.
	 */
        usecdelay(SRSP_MC_ASIC_TIME); 
        GLOBAL_INTERRUPT_BLOCK(status);
	retbufhdr_inline(bp);
        GLOBAL_INTERRUPT_UNBLOCK(status);
	return 0;
    }

    /*
     * Copy data from memd buffer to system buffer.
     */
    
    usecdelay(SRSP_MC_ASIC_TIME); 
    GLOBAL_INTERRUPT_BLOCK(status);
    cbus_ok = rsp_cbus_monitored_copy_from_memd(bufp, pak->datagramstart,
                                              size);
    GLOBAL_INTERRUPT_UNBLOCK(status);

    /* Ensure that the reads are valid. */
    if (cbus_ok == FALSE) {
        datagram_done(pak);
        return -1;
    }

    /*
     * Free memd buffer.
     */
    usecdelay(SRSP_MC_ASIC_TIME); 
    GLOBAL_INTERRUPT_BLOCK(status);
    retbufhdr_inline(bp);
    GLOBAL_INTERRUPT_UNBLOCK(status);

     
    /*
     * Send system buffer to IPC core.
     */
    reg_invoke_raw_enqueue(LINK_IPC, pak);
    return 0;
}

/*
 * Command queue polling routine.
 */
static int ipc_proc_init_command (void)
{
    bufhdr_t *bp;
    ulong tx_memd_offset;
    init_cmd_block_t *data_ptr;
    ipc_card_t *cardp;
    ipc_platform_info *ipc_platform = &ipc_platform_struct;
    ulong status;

    /*
     * MC_ASIC workaround.
     */
    usecdelay(SRSP_MC_ASIC_TIME); 
    GLOBAL_INTERRUPT_BLOCK(status);
    bp = hwq_dequeue(ipcp->cmdq);
    if (rsp_cbus_check_read() == FALSE) {
        GLOBAL_INTERRUPT_UNBLOCK(status);
	return -1;
    }

    /*
     * There is a bug in gcc compiler, that can cause it to generate
     * a branch instruction in a branch delay slot here!!.
     * branch instruction in a branch delay slot is invalid and
     * can cause undefined behaviour for the r4k series CPUs.
     * In this case, it caused the PC to jump to some other
     * routine, causing Cbus errors and undesirable effects.
     *
     * Workaround: Insert a "nop" instruction here to get around
     * gcc bug.  Cygnus is working on this bug; when gcc gets
     * fixed, visit here and remove this nop instruction.
     */
    asm volatile(" nop");
 
    if (bp) {
	if (cbus_monitor_debug)
	    buginf("\nReceive cmd from cmd queue (%x)", (uint)ipcp->cmdq);
	/*
	 * Invalidate the buffer header so we get
	 * fresh data from the data cache.
	 */
	inval_buf_hdr_inline(bp);

	/*
	 * Get pointer to init command block.
         * (Ensure that we read a valid pointer.)
	 */
	tx_memd_offset = bp->bf_buffer_ptr;
        if (rsp_cbus_check_read() == FALSE) {
            GLOBAL_INTERRUPT_UNBLOCK(status);
            return -1;
        }

	data_ptr = (init_cmd_block_t *)
	    MEMD_OFFSET_TO_UNCACHED_ADDR(tx_memd_offset);

	/*
	 * Save info in init command to our structure.
	 */
	ipcp->seat_id = data_ptr->slave_seat_id;
	ipcp->master_ctrl_port_id = data_ptr->master_ctrl_port_id;

	ipcp->master_rcv_hwq = QASIC_OFFSET_TO_ADDR(data_ptr->master_rcv_hwq);
	ipcp->self_rcv_hwq = QASIC_OFFSET_TO_ADDR(data_ptr->slave_rcv_hwq);
	ipcp->ipc_free_hwq = QASIC_OFFSET_TO_ADDR(data_ptr->ipc_free_hwq);

        /* Ensure that we read valid data. */
        if (rsp_cbus_check_read() == FALSE) {
            GLOBAL_INTERRUPT_UNBLOCK(status);
            return -1;
        }
	ipcp->valid = TRUE;

	/*
	 * Free this buffer and its header.
	 */
	retbufhdr_inline(bp);
        GLOBAL_INTERRUPT_UNBLOCK(status);

	if (ipcp->own_transport == NULL) {
	    /*
	     * Build a transport layer structure.
	     */
	    cardp = malloc(sizeof(ipc_card_t));
	    if (cardp == NULL)
		return 0;

	    cardp->seat_id = ipc_portid_to_seat(ipcp->master_ctrl_port_id);
	    cardp->hwq = ipcp->master_rcv_hwq;
	    sprintf(cardp->name, "Master IPC");

	    ipcp->own_transport = cardp;

	    /*
	     * Update platform structure so we can
	     * have our ports renumbered and master
	     * seat created.
	     */
	    ipc_globals->is_ipc_master = FALSE;
	    ipc_platform->platform_seat = ipcp->seat_id;
	    ipc_platform->platform_seat_name = "Slave RSP";
	    ipc_globals->zone_manager_pid = ipcp->master_ctrl_port_id;
	    ipc_platform->master_control_pid = ipcp->master_ctrl_port_id;
	    ipc_platform->platform_tx_vector = ipc_cbus_transmit;
	    ipc_platform->type = IPC_SEAT_CYBUS_RSP;
	    ipc_platform->transport.ipc_rsp_transport = (void *)cardp;
	    ipc_platform->do_delay_initialize = TRUE;
	} else {
	    cardp = ipcp->own_transport;
	    cardp->seat_id = ipc_portid_to_seat(ipcp->master_ctrl_port_id);
	    cardp->hwq = ipcp->master_rcv_hwq;
	    sprintf(cardp->name, "Master IPC");
	}

	/*
	 * Send a registration message back to the master.
	 */
	send_reg_message();

	/*
	 * We will start polling the incoming
	 * message queue.
	 */
	ipc_proc_fn = ipc_proc_message;
	if (cbus_monitor_debug) {
	    buginf("\nStart polling rcv hwq next (%x)",
		(uint)ipcp->self_rcv_hwq);
	}
    }
    else { 
        GLOBAL_INTERRUPT_UNBLOCK(status);
    } 
    return 0;
}

/*
 * This process performs what the function pointer points to.
 * At the first phase, it's the polling on the command queue
 * for init command from RSP master.
 * Once such command has been received and processed,
 * it starts polling the IPC message queue.
 */
static forktype ipc_slave_cbus_process (void)
{
    ulong major, minor;

    /*
     * Do not poll our command queue until
     * it is available.
     */
    while (!touch_device((void *)&ipcp->cmdq->alternate))
	;

    /*
     * Associate this process with the watched boolean
     * so we can be waken up while sleeping.
     */
    process_watch_boolean(ipc_process_wakeup, ENABLE, RECURRING);

    for (;;) {
	if (ipc_proc_fn) {
            /*
             * If something happened to the cbus, we need to sleep and
             * someone will come back wake me up to re-do all the init
             * procedure.
             */
            if (ipc_cbus_error_notice || ((*ipc_proc_fn)() < 0)) {
		ipc_proc_fn = ipc_proc_init_command;
		if (cbus_monitor_debug) {
		    buginf("\nCBus access failed, sleep now ...");
		    buginf("\nWhen wake up, start polling cmd queue next (%x)",
			(uint)ipcp->cmdq);
		}
		/*
		 * Put the process to sleep but
		 * be waken up upon change of boolean.
		 */
		process_wait_for_event();

		/*
		 * We are now waken up, first thing is
		 * to clear the boolean.
		 */
		process_set_boolean(ipc_process_wakeup, FALSE);
                ipc_cbus_error_notice = FALSE;

		/*
		 * Clear seat seq number to sync with master.
		 */
		ipc_reset_seat(ipc_get_seat(ipc_portid_to_seat
					    (ipcp->seat_id)));
		ipc_reset_seat(ipc_get_seat(ipc_portid_to_seat
					    (ipcp->master_ctrl_port_id)));
		if (cbus_monitor_debug)
		    buginf("\nCBus is told to be up again");
		/*
		 * Then check who wakes me up.
		 */
		while (process_get_wakeup(&major, &minor))
                    ;
	    }
	}
	process_suspend();
    }
}

/*
 * For slave, do nothing.
 */
void invoke_ipc_init (void)
{
    return;
}

/*
 * For slave, do nothing.
 */
void populate_txq_txacc_to_ips_body (void)
{
    return;
}

/*
 * Called from ctrlr_analyze().
 * For slave, do nothing.
 */
void ipc_cbus_find_card (slottype *slotp)
{
    return;
}

static void
ipc_cbus_init (void)
{
    int pid;
    int qindex;

    memset(ipcp, 0, sizeof(rsp_slave_ipc_rec_t));

    ipc_proc_fn = ipc_proc_init_command;

    ipc_cbus_error_notice = FALSE;

    qindex = get_hwq(QT_CMD, get_rsp_self_slot());
    ipcp->cmdq = queue_ptr(qindex);

    ipc_process_wakeup = create_watched_boolean("IPC wakeup", 0);

    /*
     * Fork a process to poll our command queue
     * to process init command from RSP Master.
     */
    pid = cfork((forkproc *)ipc_slave_cbus_process, 0, 0,
	"IPC Slave CBus process", 0);
}

/*
 * Called inside RSP Event handler.
 * For slave, do nothing.
 */
void ipc_cbus_input (void)
{
    return;
}

int ipc_cbus_num_of_cards (void)
{
    return 1;
}

static void
srsp_ipc_cbus_subsys_init (subsystype* subsys)
{
    ipc_cbus_init();

    reg_add_cbus_available_notify(cbus_available_notify,
	"cbus available notify");
    reg_add_cbus_unavailable_notify(cbus_unavailable_notify,
	"cbus unavailable notify");
}


/*
 * Send an IPC message out from RSP via CBUS to IP.
 * This routine is the send_vector for the seat
 * we're transmitting to.
 * Do not free the original message, caller will handle it.
 * Return TRUE if failed. FALSE if everything is ok.
 *
 * This has delays in it to work around HSA phase 1 bug. 
 */
boolean ipc_cbus_transmit (ipc_message *message, ipc_seat_data *seatp)
{
    int size;
    ipc_seat seat_id;
    ipc_card_t *cardp;
    bufhdr_t *bp;
    ulong tx_memd_offset;
    uchar *tx_data_ptr;
    ulong status;
    boolean cbus_ok;

    /*
     * Make sure the message size is what we can handle.
     */
    if (message == NULL)
	return TRUE;
    size = message->header->size + IPC_MESSAGE_ENCAPS_SIZE;
    if (size > RSP_MAX_IPC_BUFF_SIZE) {
	errmsg(&msgsym(INVNUM, IPC_RSP_CBUS), "Datagram size too large", size);
	return TRUE;
    }

    /*
     * Find the card structure by the seat number.
     */
    if (seatp == NULL) {
	errmsg(&msgsym(SEATERR, IPC_RSP_CBUS),
	    "Can not find card by NULL seat", (int)seatp);
	return TRUE;
    }
    seat_id = seatp->seat;
    cardp = (ipc_card_t *)seatp->transport.ipc_rsp_transport;
    if (cardp == NULL) {
	errmsg(&msgsym(SEATERR, IPC_RSP_CBUS),
	    "Can not find card by seat number", seatp->seat);
	return TRUE;
    }
    if (cardp->hwq == NULL) {
	errmsg(&msgsym(NOHWQ, IPC_RSP_CBUS), cardp->slot_num);
	return TRUE;
    }

    /*
     * Allocate a memd buffer using master/slave routine.
     */

    /*
     * mc qa_asic bug for master and slave
     * Delay, shields up, access across bus 
     */
    usecdelay(SRSP_MC_ASIC_TIME);
    GLOBAL_INTERRUPT_BLOCK(status);
    bp = alloc_ipc_memd_buffer(cardp->control);
    if (bp == NULL) {
        GLOBAL_INTERRUPT_UNBLOCK(status);
        if (rsp_cbus_check_read() == TRUE)
	    errmsg(&msgsym(NOBUF, IPC_RSP_CBUS),
	        "No more IPC memd buffers to transmit IPC message");
	return TRUE;
    }

    /*
     * Fill in buffer header. Copy data to memd buffer.
     * (Ensure that we read a valid buffer pointer.)
     */
    tx_memd_offset = bp->bf_buffer_ptr;
    if (rsp_cbus_check_read() == FALSE) {
        if (cbus_monitor_debug)
            buginf("\nIPC transmit failed on Cbus read of buffer header");
        GLOBAL_INTERRUPT_UNBLOCK(status);
        return TRUE;
    }

    bp->bf_tx_ptr = tx_memd_offset;
    bp->bf_tx_bytes = size;
    tx_data_ptr = MEMD_OFFSET_TO_UNCACHED_ADDR(tx_memd_offset);
    cbus_ok = rsp_cbus_monitored_copy_to_memd(message->header, tx_data_ptr,
                                              size);

    /*
     * Ensure that the writes to MEMD have completed successfully before
     * enqueuing the packet.
     */
    if (cbus_ok == FALSE) {
        if (cbus_monitor_debug)
            buginf("\nIPC transmit failed on Cbus write of data.");
        GLOBAL_INTERRUPT_UNBLOCK(status);
        return TRUE;
    }

    
    /*
     * Enqueue the packet on the IP's transmit queue:
     */
    hwq_enqueue_inline(cardp->hwq, bp);
    GLOBAL_INTERRUPT_UNBLOCK(status);

    /*
     * Ensure that the enqueuing of the packet has completed successfully
     * so that we can return a correct result.
     */
    if (rsp_cbus_check() == FALSE) {
        if (cbus_monitor_debug)
            buginf("\nIPC transmit failed on Cbus enqueue of packet.");
        return TRUE;
    }

    /*
     * Note: for timing purposes, queue message first.  Then
     * send debug error messages.
     */
    if (rsp_ipc_debug)
	ipc_print_message_header(message, "transmit message");

    if (cbus_monitor_debug)
	buginf("\nSend ipc message to hwq %x", (uint)cardp->hwq);
    return FALSE;
}

SUBSYS_HEADER(srsp_ipc_cbus,
	      1,		/* major */
	      0,		/* minor */
	      1,		/* edit */
	      srsp_ipc_cbus_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: cbus_mci, rsp",
	      "req: ipc, rsp");
