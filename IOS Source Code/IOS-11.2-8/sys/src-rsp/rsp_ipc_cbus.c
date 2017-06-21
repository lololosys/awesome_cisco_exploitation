/* $Id: rsp_ipc_cbus.c,v 3.3.56.7 1996/08/28 13:14:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_ipc_cbus.c,v $
 *------------------------------------------------------------------
 * RSP CBUS transport-specific routines, but common for RSP
 * master and slave.
 *
 * June 1995, Yan Ke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_ipc_cbus.c,v $
 * Revision 3.3.56.7  1996/08/28  13:14:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.56.6  1996/08/15  15:32:10  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.3.56.5  1996/07/23  13:28:43  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.56.4  1996/06/06  21:42:52  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * Fix populate_txq_txacc_to_ips not defined in boot image
 * Branch: California_branch
 *
 * Revision 3.3.56.3  1996/06/04  05:04:32  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.3.56.2  1996/05/13  22:34:10  skufer
 * CSCdi56336:  Memory Parity error-HSA Slave dies--box hangs
 * Branch: California_branch
 *
 * Revision 3.3.56.1  1996/03/18  22:02:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.2  1996/03/07  10:45:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.22.1  1996/02/20  18:01:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/27  21:34:11  yanke
 * Use cbus protection mechanism to guard slave rsp from crashing
 * during cbus stalled condition.
 * Recover ipc operation on master rsp when stall condition goes away.
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 * Revision 3.2  1995/11/17  18:48:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:28:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/17  20:55:35  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/08/04  01:29:43  yanke
 * Initial commit of placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "packet.h"
#include "interface.h"
#include "sys_registry.h"

#include "../hes/if_fci.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/ucode.h"      
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/if_cmd.h"

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
#include "rsp_commands.h"
#include "qa_hw.h"
#include "rsp_fast.h"
#include "rsp_cache.h"

#include "rsp_ipc_private.h"
#include "rsp_ipc_cbus.h"

extern boolean cbus_monitor_debug;
boolean rsp_ipc_debug = FALSE;

/*
 * Global variables used by the IPC CBus driver.
 */
#define MAX_TRIES    100  /* allow up to 100mS sleep or delay before barfing */
ipc_card_t ipc_card_list[IPC_CARD_LIST_SIZE];

/*
 * This is our re-usable scratchpad buffer used to assemble a message
 * before passing to the memd copy & send routine.
 */

#define MSG_BUF_SIZE (RSP_MAX_IPC_BUFF_SIZE - sizeof(ipc_message_header))


struct {
    ipc_message_header header;
    uchar buf[MSG_BUF_SIZE];
} fast_msg;

/*
 * Send a message to every slot that speaks ipc.
 *
 * Set DO_NOT_ACK bit in message to avoid needless buffer-consuming
 * and cpu-wasting chit-chat.  Don't bother with sequence numbers or
 * source port setting.  The only bullshit that we are forced into
 * here is finding the "control port" (like there was any
 * other kind of port needed :-).
 *
 * I would like to get rid of the port crap and just deal with hw-queue
 * pointers that could be initialized during memd buffer carve.  Then
 * we could implement all of ipc with just one or two files of
 * source code.
 */
boolean rsp_ipc_send_buffer ( void )
{
    ipc_message_header *hdr = &fast_msg.header;
    ipc_card_t         *cardp;
    bufhdr_t           *bp;
    int                i, size, tries;

    if (!hdr->size)
      return(TRUE);
    
    /*
     * For every VIP slave send down this message.
     */
    for (i = 0 ; i < MAX_SLOTS; i++) {

	cardp = &ipc_card_list[i];
	if (!(cardp->control & IPC_CARD_PERMANENT_PORT))
	    continue;

	if (cardp->hwq == NULL) {
	    ipc_globals->no_hwq++;
	    continue;
	}
	
	/*
	 * XXX - later if we get around to allocating a reasonably-sized
	 * pool of small ipc buffers in addition to the 4k buffers, we
	 * could avoid potential monopolization of non-ipc memd buffers
	 * by ipc.
	 */
	size = hdr->size + sizeof(ipc_message_header);
	tries = 0;
	while ((bp = alloc_ipc_memd_buffer(cardp->control)) == NULL) {
	    
	    if ((tries += 4) > MAX_TRIES) {
		hdr->size = 0;
		return(FALSE);
	    }
	    if (process_suspends_allowed())
		process_sleep_for(4);
	    else
		DELAY(4);
	}

	/*
	 * use PVC port. no ACK, no seq, no IPC port setup are needed
	 */
	hdr->dest_port = IPC_PERMANENT_PORT;

	/*
	 * Fill in buffer header. Copy data to memd buffer.
	 */
	bp->bf_tx_ptr = bp->bf_buffer_ptr;
	bp->bf_tx_bytes = size;
	memdcopy(hdr, MEMD_OFFSET_TO_UNCACHED_ADDR(bp->bf_buffer_ptr), size);
	hwq_enqueue_inline(cardp->hwq, bp);
	ipc_globals->fast_dfs_buffers++;
    }
    hdr->size = 0;
    return(TRUE);
}

/*
 * LATER: consider buffering in dram if we can't get memd buffers.
 */
void ip_ipc_flush_msg (void)
{
    rsp_ipc_send_buffer();
}

void *ip_ipc_get_msg (int size)
{
    size = (size + 3) & ~3;
    if ((fast_msg.header.size + size) > MSG_BUF_SIZE)
	rsp_ipc_send_buffer();
    
    return(void *)&fast_msg.buf[fast_msg.header.size];
}

void ip_ipc_send_msg (int size, ipc_message_type type, boolean flush)
{
    size = (size + 3) & ~3;
    fast_msg.header.size += size;
    fast_msg.header.type = type;
    ipc_globals->fast_dfs++;

    if (flush)
        rsp_ipc_send_buffer();
}
/*
 * Add a seat structure to IPC core with given name, send
 * function, and pointer to transport.
 */
boolean ipc_add_named_seat (uchar *seat_name, ipc_seat new_seat,
	ipc_transport_type transport,
	ipc_send_vector send_routine,
	uint rsp_transport)
{
    ipc_seat_data *new_seat_data;
 
    if (transport == IPC_SEAT_CYBUS_RSP) {

	new_seat_data = ipc_create_seat(new_seat, seat_name);
	if (new_seat_data == NULL)
	    return TRUE;
 
	new_seat_data->seat = new_seat;
	new_seat_data->send_vector = send_routine;
	new_seat_data->interrupt_ok = FALSE;

	new_seat_data->type = transport;
	new_seat_data->transport.ipc_rsp_transport = (void *)rsp_transport;

	ipc_add_seat(new_seat_data);
	return FALSE;
    }
    return TRUE;
}


/*
 * Fetch a memd buffer from the given h/w queue,
 * then allocate a system pak and copy to it.
 */
paktype *ipc_cbus_get_pak (qa_hwq_t *qptr)
{
    bufhdr_t *bp;
    int size;
    paktype *pak;
    char *bufp;

    /*
     * Fetch a buffer if there is one.
     * Otherwise bail out.
     */
    if (qptr == NULL)
	return NULL;
    bp = hwq_dequeue(qptr);
    if (bp == NULL)
	return NULL;

    /*
     * Invalidate the buffer header so we get
     * fresh data from the data cache.
     */
    inval_buf_hdr_inline((bufhdr_t *)bp);

    if (cbus_monitor_debug)
	buginf("\nReceive ipc message %x from hwq %x", (uint)bp, (uint)qptr);

    /*
     * Get a free system buffer of appropriate size.
     * If can not, drop the memd buffer and bail out.
     * We use the bf_tx_bytes as this buffer isn't
     * given to us by microcode. It contains the
     * same content as it was transmitted.
     */
    size = bp->bf_tx_bytes;
    pak = getbuffer(size);
    if (pak == NULL) {
	retbufhdr_inline(bp);
	return NULL;
    }

    /*
     * Copy data from memd buffer to system buffer, then invalidate cache
     * data lines for the MEMD data buffer, and finally, send system buffer
     * to IPC core and free the MEMD data buffer.
     */
    bufp = (char *)MEMD_OFFSET_TO_ADDR(bp->bf_buffer_ptr);
    bcopy(bufp, pak->datagramstart, size);
    invalidate_data_inline(bufp, size);

    retbufhdr_inline(bp);
    return pak;
}

/*
 * Print the content of an IPC message header.
 * For debugging purposes.
 */
void ipc_print_message_header (ipc_message *message, char *string)
{
    buginf("\nRSP_IPC: %s", string);
    buginf("\n    destination port = %x", message->header->dest_port);
    buginf("\n    source port = %x", message->header->source_port);
    buginf("\n    sequence = %x", message->header->sequence);
    buginf("\n    type = %x", message->header->type);
    buginf("\n    size = %x", message->header->size);
    buginf("\n    flags = %x", message->header->flags);
}

/*
 * This routine is called from Event interrupt handler.
 * It drains the ipc h/w queue and copies the message
 * to system buffer, then delivers it to the IPC CBus
 * process for processing.
 */
void ipc_cbus_poll (qa_hwq_t *qptr, queuetype *pakq)
{
    int msg_per_poll = NETINT_IPC_MSG_PER_POLL;
    paktype *pak;

    /*
     * Drain up to pre-defined many buffers
     * from my receive queue.
     */
    while (msg_per_poll-- > 0) {
	pak = ipc_cbus_get_pak(qptr);
	if (pak)
	    pak_enqueue(pakq, pak);
    }
}

/*
 * Enable/Disable h/w loopback at local IPC seat.
 */
int ipc_set_hw_loopback (boolean on)
{
    ipc_seat_data *new_seat;
    static ipc_send_vector orig_send_vector = NULL;

    new_seat = ipc_get_seat(ipc_globals->local_seat_addr);
    if (new_seat == NULL) {
	errmsg(&msgsym(SEATERR, IPC_RSP_CBUS),
	    "Local seat %d does not exist", ipc_globals->local_seat_addr);
	return -1;
    }

    if (orig_send_vector == NULL)
	orig_send_vector = new_seat->send_vector;

    if (on) {
	new_seat->send_vector = ipc_cbus_transmit;
	new_seat->transport.ipc_rsp_transport = get_own_transport();
    } else {
	new_seat->send_vector = orig_send_vector;
	new_seat->transport.ipc_rsp_transport = NULL;
    }
    return 0;
}

void populate_txq_txacc_to_ips (void)
{
    populate_txq_txacc_to_ips_body();
}
