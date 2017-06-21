/* $Id: ipc_cbus_rp.c,v 3.5.28.4 1996/07/23 13:24:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/ipc/ipc_cbus_rp.c,v $
 *------------------------------------------------------------------
 * RP CBUS transport-specific routines
 *
 * March 1995, Don Lobete
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipc_cbus_rp.c,v $
 * Revision 3.5.28.4  1996/07/23  13:24:40  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.5.28.3  1996/06/10  04:52:00  dlobete
 * CSCdi45148:  Error messages on VIP not reported on RP/RSP
 * Branch: California_branch
 *   - Use a more efficient scheme for slave-log port-name generation at
 *           both RP/RSP and VIP
 *   - Pull slave application details out of IPC CORE files
 *
 * Revision 3.5.28.2  1996/05/21  09:52:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.28.1  1996/03/18  20:18:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.4.1  1996/03/07  09:47:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/13  03:09:50  dlobete
 * CSCdi44696:  OIR and IPC troubles - selective sequence number synch
 * needed
 * Add slot granularity to IPC seat reset.
 *
 * Revision 3.4  1995/12/17  18:30:09  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/30  04:17:06  dlobete
 * CSCdi44691:  More bullet-proofing required for IPC slave discovery in
 * c7000
 *
 * Revision 3.2  1995/11/17  09:37:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:59:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:05:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/17  20:51:43  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.3  1995/07/13  00:12:53  dlobete
 * CSCdi34151:  UDP transport non-operational
 *
 * Revision 2.2  1995/06/28  09:27:07  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:02:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "interface_private.h"
#include "../if/network.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/cbus_registry.h"

#include "ipc_util.h"
#include "ipc.h"
#include "ipc_private.h"
#include "ipc_message.h"
#include "ipc_seats.h"
#include "ipc_ports.h"
#include "ipc_test.h"
#include "ipc_cbus.h"
#include "ipc_slave_apps.h"


#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "ipc_debug.h"

static char slot_name_array[CBUS2_MAX_CTRLR_INTERFACES][IPC_MAX_SEAT_NAME] = {
SLOT_0_NAME,
SLOT_1_NAME,
SLOT_2_NAME,
SLOT_3_NAME,
SLOT_4_NAME };

static ipc_seat slot_seat_array[CBUS2_MAX_CTRLR_INTERFACES] = {
SLOT_0_SEAT,
SLOT_1_SEAT,
SLOT_2_SEAT,
SLOT_3_SEAT,
SLOT_4_SEAT };

/*
 * Send an IPC message out form RP thru CBUS to IP.
 * This routine is the send_vector for the seat we're transmitting to.
 */

static boolean ipc_cbus_transmit_to_ip (ipc_message *message, 
						ipc_seat_data *seat)
{
    paktype *pak;
    hwidbtype *idb;
    leveltype status;                   /* lock out interrupts */
    boolean found_idb=FALSE;

    /* grab a system buffer */
    pak = getbuffer(message->header->size+ IPC_MESSAGE_ENCAPS_SIZE);
   if (pak == NULL)
	return TRUE;

    /* 
     * Prepare the pak data:
     * 		(1) if OOB must set linktype properly; 
     *		(2) insert our "special" encap value for the IP's reference
     *		(3) set the output interface
     */

    if (is_ipcclassB(message->header->dest_port))
	pak->linktype = LINK_IPC;

    FOR_ALL_HWIDBS(idb) {
	if (idb->slot == seat->transport.cbus.slot_number 
		&& idb->mci_index < 40 /* VIP TODO - remove when SPucode OK*/) {
	    found_idb = TRUE;
	    if (idb->firstsw)
	        pak->if_output = idb->firstsw;
	    else {
		datagram_done(pak);
		return TRUE;
	    }
	    break;
	}
    }

    if (!found_idb) {
	buginf("No hwidb for slot %d found!\n", 
		seat->transport.cbus.slot_number);
	datagram_done(pak);
	return(TRUE);
    }

    /* move the data into place */
    bcopy(message->header, pak->datagramstart,
	message->header->size + IPC_MESSAGE_ENCAPS_SIZE);
    pak->datagramsize = IPC_MESSAGE_ENCAPS_SIZE +
				message->header->size;

    /* Send it out down to the cbus */
    status = raise_interrupt_level(NETS_DISABLE);
    (*idb->oqueue)(idb, pak, TAIL);
    reset_interrupt_level(status);

    /* kick the output path */
    (*idb->soutput)(idb);

    return FALSE;
}

static void ipc_initialize_slaves (ulong slot)
{
    ipc_seat_data *seat_data;
    hwidbtype *idb;
    boolean found=FALSE;
    mcitype *cbus;
    dcitype *dci;

    /*
     * Find an idb for this slot.
     */
    FOR_ALL_HWIDBS(idb) {
        if (idb->slot == slot) {
	    found = TRUE;
	    break;
	}
    }
    if (!found)
	return;

    /* 
     * If not an idb representing a CIP, RVIP or SVIP,
     * don't add an IPC seat.
     */
    cbus = (mcitype *)idb->devctl;
    if (!cbus)
        return;
    dci = (dcitype *)cbus->dci[idb->slot];
    if (!dci)
        return;
 
    if (idb->type != IDBTYPE_CIP &&
            dci->ctrlr_type != FCI_RVIP_CONTROLLER &&
            dci->ctrlr_type != FCI_SVIP_CONTROLLER)
	return;

    /*
     * Set up our path to the slave if we don't already have one.
     */
    seat_data = ipc_get_seat(slot_seat_array[slot]);
    if (!seat_data)
        ipc_add_named_seat(slot_name_array[slot], slot_seat_array[slot],
                                IPC_SEAT_CXBUS_CIP,
                                ipc_cbus_transmit_to_ip,
                                (uint)slot);
    else
        ipc_reset_seat(seat_data);
 
    /*
     * Send down command to card to tell it its identity
     */
    reg_invoke_send_ipc_slave_cmd(idb, slot_seat_array[slot],
                                        ipc_globals->my_server_port);
}

static void ipc_cbus_subsys_init (subsystype *subsys)
{
    ipc_platform_info *ipc_platform = &ipc_platform_struct;

    ipc_platform->platform_tx_vector = ipc_cbus_transmit_to_ip;
    ipc_platform->platform_init_slaves_vector = ipc_initialize_slaves;

    reg_add_media_sniff_ipc(media_sniff_ipc, 
			"media_sniff_ipc");

    ipc_open_slave_log_ports();
    ipc_open_slave_core_ports();

}

boolean ipc_add_named_seat (uchar *seat_name, ipc_seat new_seat,
                                        ipc_transport_type transport,
                                        ipc_send_vector send_routine,
                                        uint slot)
{
    ipc_seat_data *new_seat_data;
 
    new_seat_data = ipc_create_seat(new_seat, seat_name);
    if (new_seat_data == NULL) return TRUE;
 
    new_seat_data->type = transport;
    if ((transport == IPC_SEAT_CXBUS_CIP)
        || (transport == IPC_SEAT_CXBUS_VIP))
        new_seat_data->transport.cbus.slot_number = (ushort)slot;
    new_seat_data->send_vector = send_routine;
    new_seat_data->interrupt_ok = FALSE;
    new_seat_data->seat = new_seat;
    ipc_add_seat(new_seat_data);
 
    return FALSE;
}

/*
 * ipc_cbus_rx
 */
void ipc_cbus_rx (hwidbtype *hwidb)
{
    paktype *pak;
    mcitype *cbus;
    mciregtype *regptr;
    ulong bytes;
    int i, *j;
 
 
    cbus = hwidb->devctl;
    regptr = cbus->regptr;
    bytes = regptr->rxlength;
 
    if (ipc_debug_packets) 
        buginf("\n%s: IPC CBUS message received, bytes %d",
                hwidb->hw_namestring, bytes);
 
    pak = getbuffer(bytes);
    if (pak == NULL) {
        return;
    }
 
    mci2buffer(&cbus->regptr->readlong, pak->datagramstart, bytes);
 
    if (ipc_debug_packets) {
        j = (int *)pak->datagramstart;
        buginf("\nIPC msg size is %d", bytes);
        for ( i=0; i<=bytes; i+=16, j+=4)
          buginf("\n%04x %08x %08x %08x %08x", i, *j, *(j+1), *(j+2), *(j+3));
    }
 
    reg_invoke_raw_enqueue(LINK_IPC, pak);
}

/*
 * media_sniff_ipc
 */
boolean media_sniff_ipc (mcitype *cbus, hwidbtype *hwidb,
					ushort offset_to_sniff)
{
    ushort result;
    mciregtype *regptr = cbus->regptr;
 
    regptr->argreg = offset_to_sniff;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    result = regptr->readshort;
    if (result == 0xbead) {
        ipc_cbus_rx(hwidb);
        return(TRUE);
    }
    return(FALSE);
}

#define IPC_CBUS_MAJOR_VERSION 1
#define IPC_CBUS_MINOR_VERSION 0
#define IPC_CBUS_EDIT_VERSION 0

SUBSYS_HEADER(ipc_cbus,
	      IPC_CBUS_MAJOR_VERSION,
	      IPC_CBUS_MINOR_VERSION,
	      IPC_CBUS_EDIT_VERSION,
	      ipc_cbus_subsys_init, SUBSYS_CLASS_PROTOCOL,
	      "req: ipc, cbus_mci", NULL);
