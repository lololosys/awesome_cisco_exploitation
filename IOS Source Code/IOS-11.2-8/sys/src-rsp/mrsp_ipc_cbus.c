/* $Id: mrsp_ipc_cbus.c,v 3.7.6.11 1996/08/28 13:13:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/mrsp_ipc_cbus.c,v $
 *------------------------------------------------------------------
 * RSP CBUS transport-specific routines at master side.
 *
 * June 1995, Yan Ke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mrsp_ipc_cbus.c,v $
 * Revision 3.7.6.11  1996/08/28  13:13:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.6.10  1996/08/15  15:31:57  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.7.6.9  1996/08/12  06:12:36  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.7.6.8  1996/06/20  15:40:32  skufer
 * CSCdi60833:  rsp HSA does not work with CIP
 * Branch: California_branch
 *         Added CIP as a supported interface processor with HSA (dual
 *         rsp in 7507 or 7513).  While the bonnet's up, changed the
 *         error message to better inform customers how to proceed.
 *
 * Revision 3.7.6.7  1996/06/10  04:52:47  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *
 * Revision 3.7.6.6  1996/06/09  23:15:21  skufer
 * CSCdi57905:  HSA needs error message when customer installs VIP or CIP
 * Branch: California_branch
 *     When HSA (two RSP's in a 7507 or 7513) is enabled, it currently
 *     cannot coexist with CIP, RVIP or SVIP adapters.  A function was
 *     introduced to detect this condition, print out an informative
 *     error message and crash the system (thereby preventing operation
 *     with both a second RSP and aforementioned xIP).  This will be
 *     removed when IPC allows these to coexist.
 *
 * Revision 3.7.6.5  1996/06/06  21:42:47  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * Fix populate_txq_txacc_to_ips not defined in boot image
 * Branch: California_branch
 *
 * Revision 3.7.6.4  1996/06/04  05:04:26  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.7.6.3  1996/05/13  22:34:09  skufer
 * CSCdi56336:  Memory Parity error-HSA Slave dies--box hangs
 * Branch: California_branch
 *
 * Revision 3.7.6.2  1996/03/21  23:49:32  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.7.6.1  1996/03/18  22:01:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:34:21  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/21  20:36:25  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Also integrate patch from dlobete to allow dfs across mic rel
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/12/11  19:24:02  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.7  1996/03/05  04:01:46  kao
 * CSCdi50679:  IPC fast message is broadcasted to all IPC-capable boards
 *
 * Revision 3.6  1996/02/29  05:22:35  foster
 * CSCdi47230:  HSA slave should come up after crash
 *
 * Revision 3.5  1996/02/28  09:33:44  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.4  1996/02/23  08:18:16  dlobete
 * CSCdi47188:  micro reload on RSP halts IPC traffic to VIPs
 * Reset sequence numbers in Master's seat structure when we initialize
 * IPC slaves.
 *
 * Revision 3.3  1995/11/27  21:34:02  yanke
 * Use cbus protection mechanism to guard slave rsp from crashing
 * during cbus stalled condition.
 * Recover ipc operation on master rsp when stall condition goes away.
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 * Revision 3.2  1995/11/17  18:46:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:27:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/26  21:32:48  motto
 * Decouple IPC from RSP subsystem.
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.2  1995/09/17  20:55:10  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/08/04  01:34:07  yanke
 * Initial commit of placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "../src-rsp/msg_qa.c"		/* Not a typo, see logger.h */
#include "packet.h"
#include "interface.h"
#include "sys_registry.h"
#include "../hes/cbus_registry.h"
#include "subsys.h"
#include "../if/network.h"

#include "../hes/if_fci.h"
#include "../hes/ccb_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/ucode.h"      
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"

#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"
#include "../ipc/ipc_slave_apps.h"

#include "rsp_bufhdr.h"
#include "rsp_chasint.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "rsp_pcmap.h"
#include "qa_hw.h"
#include "rsp_fast.h"
#include "rsp_commands.h"
#include "rsp_init.h"

#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_rsp_ipc.h"

#include "rsp_ipc_private.h"
#include "rsp_ipc_cbus.h"
#include <ciscolib.h>

/*
 * make self slot to be one bigger than valid
 * slot numbers so there is not conflicts.
 */
static const uchar rsp_self_slot = MAX_SLOTS;

/*
 * Structure contains global information about CBus driver.
 */
typedef struct ipc_rec_ {
    int num_turbo_ipc_present;	/* number of cards using turbo ipc */
    int num_classic_ipc_present; /* number of cards using classic ipc */
    int num_ipc_registered;	/* number of IPC cards that have registered */
    ipc_port_id init_port_id;	/* control port id for init message */
    queuetype ipc_msg_queue;	/* queue for incoming messages */
    qa_hwq_t *self_rcv_hwq;	/* self's rcv hw queue */
    qa_hwq_t *ipc_free_hwq_turbo;	/* global free buffer queue */
    qa_hwq_t *ipc_free_hwq_classic; /* ipc global free buffer queue CIP|RSP */
} ipc_rec_t;

/*
 * Global variables used by the IPC CBus driver.
 */
static ipc_rec_t ipc_rec;
static ipc_rec_t *ipcp = &ipc_rec;
/*
 * one second delay if bad ipc configuration
 */
#define ONE_SECOND_PAUSE 1

extern boolean cbus_monitor_debug;

/*
 * Allocate a memd buffer from IPC global
 * free queue.
 */
bufhdr_t *alloc_ipc_memd_buffer (ulong ipc_control)
{
      return get_ipc_memd_buffer(ipc_control); 
}

/*
 * Return the card structure of my own.
 */
ipc_card_t *get_own_transport (void)
{
    return &ipc_card_list[rsp_self_slot];
}

/*
 * Find the currently discovered IPC card structure
 * that matches the given seat id.
 */
static ipc_card_t *find_card_by_seat (ipc_seat seat_id)
{
    int slot_num;
    ipc_card_t *cardp;

    for (slot_num = 0; slot_num < MAX_SLOTS; slot_num++) {
	cardp = &ipc_card_list[slot_num];
	if (cardp->seat_id == seat_id && (cardp->control & IPC_CARD_PRESENT))
	    return cardp;
    }
    return NULL;
}

/*
 * Process received IPC init messages.
 * This is a registered callback routine on the IPC Init port.
 * A new seat is created for each registered back IPC card.
 * Need to free the message when done.
 */
static void proc_reg_message (ipc_message *message,
	void *context,
	ipc_error_code error)
{
    ipc_cbus_reg_msg_t *regp;
    ipc_seat seat_id;
    ipc_card_t *cardp;
    ipc_seat_data *new_seat;
    boolean failure;

    if (message == NULL)
	return;
    regp = (ipc_cbus_reg_msg_t *)message->data;

    /*
     * Find the card structure for this sender.
     * Make sure this is the first time the
     * card is registering to us.
     */
    seat_id = regp->seat_id;
    cardp = find_card_by_seat(seat_id);
    if (cardp == NULL) {
	errmsg(&msgsym(SEATERR, IPC_RSP_CBUS),
	    "Can not find card by seat id", cardp->seat_id);
	ipc_return_message(message);
	return;
    }
    if (cardp->control & IPC_CARD_REGISTERED) {
	errmsg(&msgsym(CARDERR, IPC_RSP_CBUS),
	    "Card has already been registered");
	ipc_return_message(message);
	return;
    }

    cardp->control |= IPC_CARD_REGISTERED;
    ipcp->num_ipc_registered++;

    /*
     * Create a seat for this ipc card.
     * Make sure we don't duplicate seat.
     */
    new_seat = ipc_get_seat(seat_id);
    if (new_seat) {
	errmsg(&msgsym(SEATERR, IPC_RSP_CBUS),
	    "Seat %d already exists", cardp->seat_id);
	ipc_return_message(message);
	return;
    }

    failure = ipc_add_named_seat(cardp->name, seat_id,
	IPC_SEAT_CYBUS_RSP, ipc_cbus_transmit, (uint)cardp);
    if (failure == TRUE) {
	errmsg(&msgsym(SEATERR, IPC_RSP_CBUS),
	    "Can not create seat by seat id", cardp->seat_id);
    }

    ipc_return_message(message);
    return;
}

/*
 * Send an ipc CCB command to IP.
 */
static int send_ipc_init_command_generic (int cmd, ipc_card_t *cardp)
{
    slottype *slotp = &slots[cardp->slot_num];

    if (cmd == CCB_CMD_IPC_QUEUE) {
       /* IPC pools differ if CIP or not, use 'control' field */
       if (cardp->control & IPC_CARD_PERMANENT_PORT) { 
	  send_ip_cmd_3arg(slotp, cmd,
	    (ushort)BUFHDR_ADDR_TO_OFF(cardp->hwq),
	    (ushort)BUFHDR_ADDR_TO_OFF(ipcp->self_rcv_hwq),
	    (ushort)BUFHDR_ADDR_TO_OFF(ipcp->ipc_free_hwq_turbo));
       } else { 
	  send_ip_cmd_3arg(slotp, cmd,
	    (ushort)BUFHDR_ADDR_TO_OFF(cardp->hwq),
	    (ushort)BUFHDR_ADDR_TO_OFF(ipcp->self_rcv_hwq),
	    (ushort)BUFHDR_ADDR_TO_OFF(ipcp->ipc_free_hwq_classic));
       } 
    } else if (cmd == CCB_CMD_IPC_INIT) {
	send_ip_cmd_2larg(slotp, cmd,
	    cardp->seat_id,
	    ipc_globals->my_server_port);
    } else
	return -2;
    return 0;
}

static void send_ipc_init_command_to_rsp (ipc_card_t *cardp)
{
    bufhdr_t *bufptr;
    ulong tx_memd_offset;
    init_cmd_block_t *data_ptr;
    /*
    ipc_seat seat_id;
    ipc_port_id port_id;
    */
    slottype *slotp;

    /*
     * Allocate a memd buffer from ipc buffer pool
     * to send init command.
     */
    bufptr = get_ipc_memd_buffer(cardp->control);
    if (bufptr == NULL) {
	errmsg(&msgsym(NOBUF, IPC_RSP_CBUS),
	    "No IPC memd buffers for init cmd");
	return;
    }

    /*
     * Set up buffer pointer and data size.
     */
    tx_memd_offset = bufptr->bf_buffer_ptr;
    /*
    bufptr->bf_tx_ptr = tx_memd_offset;
    bufptr->bf_tx_bytes = sizeof(init_cmd_block_t);
    */

    /*
     * Fill command parameters into memd buffer.
     */
    data_ptr = (init_cmd_block_t *)
	MEMD_OFFSET_TO_UNCACHED_ADDR(tx_memd_offset);

    data_ptr->slave_seat_id = cardp->seat_id;
    data_ptr->master_ctrl_port_id = ipc_globals->my_server_port;

    data_ptr->master_rcv_hwq = (ushort)BUFHDR_ADDR_TO_OFF(ipcp->self_rcv_hwq);
    data_ptr->slave_rcv_hwq = (ushort)BUFHDR_ADDR_TO_OFF(cardp->hwq);
    data_ptr->ipc_free_hwq = (ushort)BUFHDR_ADDR_TO_OFF(ipcp->ipc_free_hwq_classic);

    /*
     * Send this buffer to slave RSP via cmd queue.
     */
    slotp = &slots[cardp->slot_num];
    if (cbus_monitor_debug) {
	buginf("\nSend init command to slave rsp via hwq %x",
	    (uint)slotp->cmd_q_ptr);
	buginf("\n\tslave seat id = %x", data_ptr->slave_seat_id);
	buginf("\n\tmaster ctrl port id = %x", data_ptr->master_ctrl_port_id);
	buginf("\n\tmaster rcv hwq  = %x", data_ptr->master_rcv_hwq);
	buginf("\n\tslave rcv hwq  = %x", data_ptr->slave_rcv_hwq);
	buginf("\n\tipc free hwq  = %x", data_ptr->ipc_free_hwq);
    }
    hwq_enqueue(slotp->cmd_q_ptr, bufptr);
}

/*
 * Build a init command block then send it to the given card
 * For other IPC cards, the implementation could be different.
 */
static void send_ipc_init_command (ipc_card_t *cardp)
{
    /*
     * Also, reset sequence numbers in the master's seat structure.
     */
    reg_invoke_ipc_reset_seq_nums(cardp->slot_num);
 
    /*
     * Use buffer to send init command for rsp.
     * Use two CCBs to send init command for VIP.
     */
    if (cardp->slot_num == get_rsp_peer_slot()) {
	send_ipc_init_command_to_rsp(cardp);
	return;
    }

    /*
     * Also, reset sequence numbers in the master's seat structure.
     */
    reg_invoke_ipc_reset_seq_nums(cardp->slot_num);

    /*
     * If this is the boot loader image, bypass
     * initialize slave IPCs since not all required system
     * resources are available to support full IPC functionality
     * and IPs do not require IPC at bootldr time.
     * see [CSCdi59711].
     */
    if (system_loading)
        return;

    /*
     * For VIP, init command are sent by two CCB cmd.
     * First send hwq, then port/seat id info.
     */
    send_ipc_init_command_generic(CCB_CMD_IPC_QUEUE, cardp);
    send_ipc_init_command_generic(CCB_CMD_IPC_INIT, cardp);

    return;
}

static void send_ipc_init_command_to_slot (int slot_num)
{
    ipc_card_t *cardp;
    boolean failure;
    ipc_seat_data *seat_data;

    cardp = &ipc_card_list[slot_num];
    if (cardp->control & IPC_CARD_PRESENT){

	/* 
	 * For now assume the card will come up
	 * withput waiting for a register. VIP FIXME
	 */
	cardp->control |= IPC_CARD_REGISTERED;
	ipcp->num_ipc_registered++;

	cardp->seat_id = (slot_num + 0x100) << 16;
	seat_data = ipc_get_seat(cardp->seat_id);
	if (seat_data == NULL) {
	    if (cbus_monitor_debug) {
		buginf("\nFirst time creating this port (%s,%x)",
		    cardp->name, cardp->seat_id);
	    }
	    failure = ipc_add_named_seat(cardp->name,
		cardp->seat_id,
		IPC_SEAT_CYBUS_RSP, ipc_cbus_transmit, (uint)cardp);
	    if (failure == TRUE) {
		errmsg(&msgsym(SEATERR, IPC_RSP_CBUS),
		    "Can not create seat by seat id", cardp->seat_id);
	    }
	} else {
	    ipc_reset_seat(seat_data);
	    if (cbus_monitor_debug) {
		buginf("\nSeat exists already (%s,%x)",
		    cardp->name, cardp->seat_id);
	    }
	}
	send_ipc_init_command(cardp);
    }
}

/*
 * Send init command to IPC card.
 */
static void invoke_ipc_init (void)
{
    ipc_error_code retcode;
    ipc_port_id port_id;
    slottype* slotp;

    /*
     * Now is a good time to create a control port
     * for initialization and register a handler on it.
     *
     * Put in some special handling to take care of "micro rel".
     */
    retcode = ipc_create_named_port(IPC_CBUS_INIT_PORT_NAME,
				    &port_id, IPC_PORT_OOB);
    if (retcode == IPC_OK) {
	ipcp->init_port_id = port_id;
	retcode = ipc_set_receive_callback(port_id, NULL, proc_reg_message);
	if (retcode != IPC_OK) {
	    if (cbus_monitor_debug) {
		buginf("\nCan not set receive callback routine for port %d",
		       port_id);
	    }
	    return;
	}
    } else {
	if (retcode != IPC_PORT_IN_USE) {
	    if (cbus_monitor_debug) {
		buginf("\nCan not create named %s port",
		       IPC_CBUS_INIT_PORT_NAME);
	    }
	    return;
	}
    }

    /*
     * By now the memd has been carved and an IPC free buffer
     * is availabe, cache it in our global structure.
     */
    ipcp->ipc_free_hwq_turbo = IpcbufQ;
    ipcp->ipc_free_hwq_classic = IpcbufQ_classic;

    /*
     *	Send init command to each discovered ipc IP
     *	and possible slave RSP.
     */
    if (rsp_slave_state() == SLAVE_RUNNING ||
	rsp_slave_state() == SLAVE_NEEDS_BOOT)
    {
	send_ipc_init_command_to_slot(get_rsp_peer_slot());
    }

    FOR_ALL_ENABLED_IP_SLOTS(slotp) {
	send_ipc_init_command_to_slot(slotp->slot_num);
    }

    ipc_open_slave_log_ports();
    ipc_open_slave_core_ports();
}

/*
 * populate_txq_txacc_to_ips_body
 * This routine is used to populate new txq and txacc after memd carving to 
 * all IPs.
 */
void populate_txq_txacc_to_ips_body (void)
{
    hwidbtype *hwidb;
    ipc_ipfast_update_idb_msg *update_idb_msg;
    int size = sizeof(ipc_ipfast_update_idb_msg);
 
    FOR_ALL_HWIDBS(hwidb) {
 
        update_idb_msg = ip_ipc_get_msg(size);
        update_idb_msg->hdr.msg_size = size;
        update_idb_msg->hdr.ipfast_type = IPFAST_UPDATE_IDB;
        update_idb_msg->hw_if_index = hwidb->hw_if_index;
        update_idb_msg->txq_ptr = (ulong)hwidb->tx_q_ptr;
        update_idb_msg->tql_ptr = (ulong)hwidb->tx_acc_ptr;
        if (rsp_ipc_debug) {
            buginf("\nIPFAST_DS:     IPFAST_UPDATE_IDB\n");
            buginf("        hw_if_index      %x\n",
                   update_idb_msg->hw_if_index);
            buginf("        txq_ptr        %x\n", update_idb_msg->txq_ptr);
            buginf("        tql_ptr        %x\n", update_idb_msg->tql_ptr);
        }
        ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, FALSE);
    }
}

void ipc_send_hwindex_to_vips (hwidbtype *hwidb)
{
    hwidbtype *tmphwidb;
    ipc_ipfast_update_hwindex_msg *update_index_msg;
    ipc_ipfast_hwindex_elt *data_ptr;
    int size = sizeof(ipc_ipfast_update_hwindex_msg);
    ushort num_indices = 0;
 
    if (!hwidb) {
 
        /*
         * Send all hw_if_index data to all VIPs in the box.
         */
        FOR_ALL_HWIDBS(tmphwidb)
            if (IS_VIP(tmphwidb))
                num_indices++;
 
        size += (num_indices * sizeof(ipc_ipfast_hwindex_elt));
        if (size > RSP_MAX_IPC_BUFF_SIZE) {
            errmsg(&msgsym(INVNUM, IPC_RSP_CBUS), "Datagram size too large",
                   size);
            return;
        }
 
        update_index_msg = ip_ipc_get_msg(size);
        update_index_msg->hdr.msg_size = size;
        update_index_msg->hdr.ipfast_type = IPFAST_UPDATE_INDEX;
        update_index_msg->num_indices = num_indices;
        data_ptr = update_index_msg->data;
 
        if (rsp_ipc_debug) {
            buginf("\nIPFAST_DS:     IPFAST_UPDATE_INDEX\n");
            buginf("        num_indices        %x\n", num_indices);
        }
        FOR_ALL_HWIDBS(tmphwidb) {
            if (IS_VIP(tmphwidb)) {
                data_ptr->hw_if_index = tmphwidb->hw_if_index;
                data_ptr->slot = tmphwidb->slot;
                data_ptr->slotunit = tmphwidb->slotunit;
                data_ptr->vc = tmphwidb->vc;
                data_ptr++;
            }
        }
        ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
    } else {
        /*
         * One hwidb needs to be updated. Only VIPs are interested.
         */
        if (!IS_VIP(hwidb))
            return;
 
        size += sizeof(ipc_ipfast_hwindex_elt);
 
        update_index_msg = ip_ipc_get_msg(size);
        update_index_msg->hdr.msg_size = size;
        update_index_msg->hdr.ipfast_type = IPFAST_UPDATE_INDEX;
        update_index_msg->num_indices = 1;
        data_ptr = update_index_msg->data;
 
        if (rsp_ipc_debug) {
            buginf("\nIPFAST_DS:     IPFAST_UPDATE_INDEX\n");
            buginf("        num_indices        %x\n", 1);
        }
 
        data_ptr->hw_if_index = hwidb->hw_if_index;
        data_ptr->slot = hwidb->slot;
        data_ptr->slotunit = hwidb->slotunit;
        data_ptr->vc = hwidb->vc;
 
        ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
    }
}

/*
 * Return the number of ipc cards detected present.
 * This routine is used by memd carving routines.
 */
void ipc_cbus_num_of_cards ( int *num_turbo, int *num_classic)
{
    /*
     * The number of IPC capable controllers can
     * be different from the number of them registered
     * back. But always more, so this number returned
     * could be an overestimate.
     */
    *num_turbo = ipcp->num_turbo_ipc_present; 
    *num_classic = ipcp->num_classic_ipc_present;
}

/*
 * Initialize the card structure with IPC related info
 * and allocated sources.
 */
static int init_card (ipc_card_t *cardp, ulong control)
{
    int slot_num;
    int qindex;
    qa_hwq_t *qptr;

    slot_num = cardp->slot_num;

    /*
     * Allocate a h/w queue and save it in card structure.
     * If it's for us, associate it the attention and save
     * it in global structure for easy access.
     */
    qindex = get_hwq(QT_GFREE, 1);
    if (qindex == QA_ALLOC_ERROR) {
	errmsg(&msgsym(ALLOC, QA), "alloc IPC free queue failed");
	return -1;
    }
    qptr = queue_ptr(qindex);
    cardp->hwq = qptr;
    if (slot_num == rsp_self_slot) {
   	set_qa_rp_attention(qindex, QA_ATTN_CMD);
	ipcp->self_rcv_hwq = qptr;
    }

    /*
     * Update other info.
     */
    cardp->slot_num = slot_num;
    cardp->seat_id = ipc_make_class_b_seatid(slot_num);
    cardp->control |= control;

    /*
     * Name not exceeding IPC_CARD_NAME_LEN.
     */
    sprintf(cardp->name, "RSP IPC card slot %d", slot_num);

    /* 
     * cards with IPC_CARD_PERMANENT_PORT use turbo ipc.
     */
    if (cardp->control & IPC_CARD_PERMANENT_PORT)  { 
        ipcp->num_turbo_ipc_present++;
    } else { 
        ipcp->num_classic_ipc_present++;
    }
    return 0;
}

/*
 * Determine if the card on the given slot
 * supports IPC or not. Return TRUE if yes.
 */
static ulong card_ipc_support_types (slottype *slotp)
{
    switch (slotp->ctrlr_type) {

	case FCI_RSP1_CONTROLLER:
	case FCI_RSP2_CONTROLLER:
	case FCI_CIP_CONTROLLER:
	    return(IPC_CARD_PRESENT);

	case FCI_RVIP_CONTROLLER:
	case FCI_SVIP_CONTROLLER:
	    return(IPC_CARD_PRESENT | IPC_CARD_PERMANENT_PORT);

	default:
	    return 0;
    }
}
/*
 * Analyze the controller at this slot to check if
 * it's supporting IPC. Called from ctrlr_analyze()
 */
static void ipc_ctrlr_analyze (void)
{
    int slot_num;
    ipc_card_t *cardp;
    int qindex;
    slottype *slotp;

    /*
     *	The RSP master/slave needs to be taken care of.
     */
    cardp = &ipc_card_list[rsp_self_slot];
    if ((cardp->control & IPC_CARD_PRESENT) == 0) {
	cardp->slot_num = rsp_self_slot;
	init_card(cardp, IPC_CARD_PRESENT);
    }

    if (rsp_slave_state() == SLAVE_RUNNING ||
	rsp_slave_state() == SLAVE_NEEDS_BOOT)
    {
	slot_num = get_rsp_peer_slot();
	cardp = &ipc_card_list[slot_num];
	if ((cardp->control & IPC_CARD_PRESENT) == 0) {
	    cardp->slot_num = slot_num;
	    init_card(cardp, IPC_CARD_PRESENT);

	    qindex = get_hwq(QT_CMD, slot_num);
	    slots[slot_num].cmd_q_ptr = queue_ptr(qindex);
	}
    }

    FOR_ALL_ENABLED_IP_SLOTS(slotp) {
	ulong control;

	control = card_ipc_support_types(slotp);
	if (control) {
	    slot_num = slotp->slot_num;
	    cardp = &ipc_card_list[slot_num];
	    cardp->slot_num = slot_num;
	    init_card(cardp, control);
	}
    }
}

/* 
 * Check to see if we have any packets waiting on our inbound queue.
 */
static boolean ipc_cbusBLOCK (void)
{
    return (ipcp->ipc_msg_queue.qhead) ? FALSE : TRUE;
}

/*
 * IPC CBus process that fetches messages coming from
 * the IPC queue, those messages are placed there
 * by the Event interrupt handler.
 */
static forktype ipc_cbus_process (void)
{
    paktype *pak;

    for (;;) {
	edisms((blockproc *)ipc_cbusBLOCK, 0);

	while ((pak = pak_dequeue(&ipcp->ipc_msg_queue)) != NULL)
	    /*
	     *	Forward a packet to IPC core
	     *	for processing.
	     */
	    reg_invoke_raw_enqueue(LINK_IPC, pak);
    }
}

static void ipc_cbus_init (void)
{
    /*
     * Clear ipc cbus global structures.
     */
    memset(ipcp, 0, sizeof(ipc_rec_t));
    memset(ipc_card_list, 0, IPC_CARD_LIST_SIZE * sizeof(ipc_card_t));
}

/*
 * Called inside RSP Event handler.
 */
void ipc_cbus_input (void)
{
    /*
     * Polling message of our receive h/w queue
     * and send message to the driver message queue
     */
    ipc_cbus_poll(ipcp->self_rcv_hwq, &ipcp->ipc_msg_queue);
}

/*
 * Reset the sequence numbers for this card in the
 * Master's seat table.
 */
void mrsp_ipc_initialize_slaves (ulong slot)
{
    ipc_card_t *cardp;
    ipc_seat_data *seat_data;

    cardp = &ipc_card_list[slot];
    seat_data = ipc_get_seat(cardp->seat_id);
    ipc_reset_seat(seat_data);
}

static void mrsp_ipc_cbus_subsys_init (subsystype* subsys)
{
    int pid;
    ipc_platform_info *ipc_platform = &ipc_platform_struct;
 
    ipc_platform->platform_init_slaves_vector = mrsp_ipc_initialize_slaves;

    reg_add_ipc_cbus_input( ipc_cbus_input, "ipc_cbus_input");
    reg_add_ipc_cbus_num_of_cards( ipc_cbus_num_of_cards, "ipc_cbus_num_of_cards");
    reg_add_ipc_cbus_init(ipc_cbus_init, "ipc_cbus_init");
    reg_add_invoke_ipc_init(invoke_ipc_init, "invoke_ipc_init");
    reg_add_send_hwindex_to_vips(ipc_send_hwindex_to_vips,
				 "ipc_send_hwindex_to_vips");
    reg_add_ipc_ctrlr_analyze(ipc_ctrlr_analyze, "ipc_ctrlr_analyze");
    
    ipc_set_hw_loopback(TRUE);

    /*
     * Fork a process to handle incoming messages
     * from interrupt handler.
     */
    pid = cfork((forkproc *)ipc_cbus_process, 0, 0, "IPC CBus process", 0);
}

/*
 * Send an IPC message out from RSP via CBUS to IP.
 * This routine is the send_vector for the seat
 * we're transmitting to.
 * Do not free the original message, caller will handle it.
 * Return TRUE if failed. FALSE if everything is ok.
 */
boolean ipc_cbus_transmit (ipc_message *message, ipc_seat_data *seatp)
{
    int size;
    ipc_seat seat_id;
    ipc_card_t *cardp;
    bufhdr_t *bp;
    ulong tx_memd_offset;
    uchar *tx_data_ptr;

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

    /*
     * Allocate a memd buffer using master/slave routine.
     */
    bp = alloc_ipc_memd_buffer(cardp->control);
    if (bp == NULL) {
	errmsg(&msgsym(NOBUF, IPC_RSP_CBUS),
	    "No more IPC memd buffers to transmit IPC message");
	return TRUE;
    }

    /*
     * Fill in buffer header. Copy data to memd buffer.
     */
    tx_memd_offset = bp->bf_buffer_ptr;
    bp->bf_tx_ptr = tx_memd_offset;
    bp->bf_tx_bytes = size;
    tx_data_ptr = MEMD_OFFSET_TO_UNCACHED_ADDR(tx_memd_offset);
    memdcopy(message->header, tx_data_ptr, size);

    /*
     * Enqueue the packet on the IP's transmit queue:
     */
    if (cardp->hwq == NULL) {
	errmsg(&msgsym(NOHWQ, IPC_RSP_CBUS), cardp->slot_num);
	return TRUE;
    }

    if (rsp_ipc_debug)
	ipc_print_message_header(message, "transmit message");

    if (cbus_monitor_debug)
	buginf("\nSend ipc message to hwq %x", (uint)cardp->hwq);

    hwq_enqueue_inline(cardp->hwq, bp);
    return FALSE;
}

#define MRSP_IPC_CBUS_MAJOR_VERSION 1
#define MRSP_IPC_CBUS_MINOR_VERSION 0
#define MRSP_IPC_CBUS_EDIT_VERSION 0

SUBSYS_HEADER(mrsp_ipc_cbus,
	      MRSP_IPC_CBUS_MAJOR_VERSION, 
              MRSP_IPC_CBUS_MINOR_VERSION, 
              MRSP_IPC_CBUS_EDIT_VERSION,
	      mrsp_ipc_cbus_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: cbus_mci, rsp",
	      "req: ipc, rsp");
